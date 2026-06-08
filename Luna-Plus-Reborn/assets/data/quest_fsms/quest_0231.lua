--[[
  Quest 231: [Revenge] Howling Mad
  Level Required: 50
  NPC Start: 0, NPC Complete: 0
  Rewards: EXP=180000, Gold=20000
]]

local fsm = require('fsm_engine')

local quest_231 = fsm:new({
    id = 231,
    name = "[Revenge] Howling Mad",
    level_required = 50,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 231: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "accept",
                    target = "IN_PROGRESS",
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 231: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 92,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 231: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 87,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 231: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 231: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 231: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 231: Rewards given")
                self:give_rewards({
                    exp = 180000,
                    gold = 20000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_231