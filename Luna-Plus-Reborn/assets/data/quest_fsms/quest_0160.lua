--[[
  Quest 160: Orc Elimination
  Level Required: 31
  NPC Start: 29, NPC Complete: 29
  Prerequisites: [154]
  Rewards: EXP=34994, Gold=4000
]]

local fsm = require('fsm_engine')

local quest_160 = fsm:new({
    id = 160,
    name = "Orc Elimination",
    level_required = 31,
    npc_start = 29,
    npc_complete = 29,
    prerequisites = {154},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 160: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 160: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 160: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 10,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 160: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 160: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 160: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 160: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 160: Rewards given")
                self:give_rewards({
                    exp = 34994,
                    gold = 4000,
                    items = {
                        { item_id = 21000004, count = 15 },
                        { item_id = 21000095, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_160