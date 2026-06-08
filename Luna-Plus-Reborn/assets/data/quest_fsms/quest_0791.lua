--[[
  Quest 791: Multi-Target
  Level Required: 57
  NPC Start: 43, NPC Complete: 43
  Rewards: EXP=318693, Gold=13316
]]

local fsm = require('fsm_engine')

local quest_791 = fsm:new({
    id = 791,
    name = "Multi-Target",
    level_required = 57,
    npc_start = 43,
    npc_complete = 43,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 791: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 43,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 791: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 791: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 62,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 791: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 43,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 791: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 791: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 43,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 791: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 791: Rewards given")
                self:give_rewards({
                    exp = 318693,
                    gold = 13316,
                })
            end,
            transitions = {},
        },
    },
})

return quest_791