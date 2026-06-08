--[[
  Quest 947: [Party] The Truth About John and Paul
  Level Required: 0
  NPC Start: 541, NPC Complete: 541
  Prerequisites: [943]
  Rewards: EXP=14617802, Gold=101632
]]

local fsm = require('fsm_engine')

local quest_947 = fsm:new({
    id = 947,
    name = "[Party] The Truth About John and Paul",
    level_required = 0,
    npc_start = 541,
    npc_complete = 541,
    prerequisites = {943},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 947: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 947: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 947: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 327,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 947: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 332,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 947: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 336,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 947: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 947: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 947: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 947: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 947: Rewards given")
                self:give_rewards({
                    exp = 14617802,
                    gold = 101632,
                })
            end,
            transitions = {},
        },
    },
})

return quest_947