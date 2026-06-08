--[[
  Quest 161: Orc Elimination
  Level Required: 31
  NPC Start: 29, NPC Complete: 29
  Prerequisites: [160]
  Rewards: EXP=34994, Gold=4000
]]

local fsm = require('fsm_engine')

local quest_161 = fsm:new({
    id = 161,
    name = "Orc Elimination",
    level_required = 31,
    npc_start = 29,
    npc_complete = 29,
    prerequisites = {160},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 161: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 161: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 161: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 10,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 161: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 161: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 161: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 161: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 161: Rewards given")
                self:give_rewards({
                    exp = 34994,
                    gold = 4000,
                    items = {
                        { item_id = 21000095, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_161