--[[
  Quest 949: [Party] Soothing Silence 2
  Level Required: 107
  NPC Start: 542, NPC Complete: 542
  Prerequisites: [942]
  Rewards: EXP=14617802, Gold=101632
]]

local fsm = require('fsm_engine')

local quest_949 = fsm:new({
    id = 949,
    name = "[Party] Soothing Silence 2",
    level_required = 107,
    npc_start = 542,
    npc_complete = 542,
    prerequisites = {942},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 949: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 949: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 949: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 325,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 949: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 329,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 949: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 332,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 949: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 949: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 949: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 949: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 949: Rewards given")
                self:give_rewards({
                    exp = 14617802,
                    gold = 101632,
                })
            end,
            transitions = {},
        },
    },
})

return quest_949