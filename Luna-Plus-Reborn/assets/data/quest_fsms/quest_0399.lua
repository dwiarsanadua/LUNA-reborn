--[[
  Quest 399: [Quest] The wandering dead
  Level Required: 56
  NPC Start: 90, NPC Complete: 90
  Rewards: EXP=372532, Gold=12000
]]

local fsm = require('fsm_engine')

local quest_399 = fsm:new({
    id = 399,
    name = "[Quest] The wandering dead",
    level_required = 56,
    npc_start = 90,
    npc_complete = 90,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 399: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 399: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 399: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 20,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 399: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 399: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 399: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 399: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 399: Rewards given")
                self:give_rewards({
                    exp = 372532,
                    gold = 12000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_399