--[[
  Quest 450: The Legacy Fragment
  Level Required: 105
  NPC Start: 122, NPC Complete: 122
  Prerequisites: [449]
  Rewards: EXP=11327799, Gold=500000
]]

local fsm = require('fsm_engine')

local quest_450 = fsm:new({
    id = 450,
    name = "The Legacy Fragment",
    level_required = 105,
    npc_start = 122,
    npc_complete = 122,
    prerequisites = {449},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 450: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 450: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 450: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 297,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 450: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 450: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 450: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 450: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 450: Rewards given")
                self:give_rewards({
                    exp = 11327799,
                    gold = 500000,
                    items = {
                        { item_id = 326, count = 1 },
                        { item_id = 326, count = 1 },
                        { item_id = 30000541, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_450