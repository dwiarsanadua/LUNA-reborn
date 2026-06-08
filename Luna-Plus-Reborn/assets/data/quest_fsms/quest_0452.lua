--[[
  Quest 452: The Third Legacy Fragment
  Level Required: 105
  NPC Start: 122, NPC Complete: 122
  Prerequisites: [449]
  Rewards: EXP=11327799, Gold=1000000
]]

local fsm = require('fsm_engine')

local quest_452 = fsm:new({
    id = 452,
    name = "The Third Legacy Fragment",
    level_required = 105,
    npc_start = 122,
    npc_complete = 122,
    prerequisites = {449},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 452: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 452: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 452: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 299,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 452: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 452: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 452: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 452: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 452: Rewards given")
                self:give_rewards({
                    exp = 11327799,
                    gold = 1000000,
                    items = {
                        { item_id = 328, count = 1 },
                        { item_id = 30000543, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_452