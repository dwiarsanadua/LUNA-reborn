--[[
  Quest 423: [Quest] Qualification
  Level Required: 81
  NPC Start: 98, NPC Complete: 98
  Rewards: EXP=2266495, Gold=100000
]]

local fsm = require('fsm_engine')

local quest_423 = fsm:new({
    id = 423,
    name = "[Quest] Qualification",
    level_required = 81,
    npc_start = 98,
    npc_complete = 98,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 423: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 423: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 423: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 273,
                    count = 80,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 423: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 423: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 423: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 423: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 423: Rewards given")
                self:give_rewards({
                    exp = 2266495,
                    gold = 100000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_423