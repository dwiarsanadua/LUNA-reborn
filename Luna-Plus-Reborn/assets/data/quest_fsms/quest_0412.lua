--[[
  Quest 412: [Quest] A surprise attack from Spider
  Level Required: 75
  NPC Start: 97, NPC Complete: 97
  Prerequisites: [407]
  Rewards: EXP=1454523, Gold=218382
]]

local fsm = require('fsm_engine')

local quest_412 = fsm:new({
    id = 412,
    name = "[Quest] A surprise attack from Spider",
    level_required = 75,
    npc_start = 97,
    npc_complete = 97,
    prerequisites = {407},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 412: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 412: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 412: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 250,
                    count = 70,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 412: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 412: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 412: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 412: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 412: Rewards given")
                self:give_rewards({
                    exp = 1454523,
                    gold = 218382,
                })
            end,
            transitions = {},
        },
    },
})

return quest_412