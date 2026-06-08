--[[
  Quest 129: A Tour Guide
  Level Required: 150
  NPC Start: 7, NPC Complete: 10
  Rewards: EXP=377, Gold=530
]]

local fsm = require('fsm_engine')

local quest_129 = fsm:new({
    id = 129,
    name = "A Tour Guide",
    level_required = 150,
    npc_start = 7,
    npc_complete = 10,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 129: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 129: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 129: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 129: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 129: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 129: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 129: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 129: Rewards given")
                self:give_rewards({
                    exp = 377,
                    gold = 530,
                })
            end,
            transitions = {},
        },
    },
})

return quest_129