--[[
  Quest 923: [Party][Repeat] Tranquil Wetland Garden
  Level Required: 101
  NPC Start: 541, NPC Complete: 541
  Rewards: EXP=10442383, Gold=81769
]]

local fsm = require('fsm_engine')

local quest_923 = fsm:new({
    id = 923,
    name = "[Party][Repeat] Tranquil Wetland Garden",
    level_required = 101,
    npc_start = 541,
    npc_complete = 541,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 923: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 923: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 923: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 320,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 923: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 335,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 923: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 923: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 923: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 923: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 923: Rewards given")
                self:give_rewards({
                    exp = 10442383,
                    gold = 81769,
                })
            end,
            transitions = {},
        },
    },
})

return quest_923