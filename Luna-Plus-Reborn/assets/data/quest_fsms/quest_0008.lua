--[[
  Quest 8: Patrol's Mission
  Level Required: 7
  NPC Start: 13, NPC Complete: 13
  Prerequisites: [5]
  Rewards: EXP=420, Gold=1485
]]

local fsm = require('fsm_engine')

local quest_8 = fsm:new({
    id = 8,
    name = "Patrol's Mission",
    level_required = 7,
    npc_start = 13,
    npc_complete = 13,
    prerequisites = {5},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 8: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 8: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 8: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 19,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 8: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 8: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 8: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 8: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 8: Rewards given")
                self:give_rewards({
                    exp = 420,
                    gold = 1485,
                    items = {
                        { item_id = 3, count = 1 },
                        { item_id = 3, count = 10 },
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_8