--[[
  Quest 407: [Quest] A village in ruins
  Level Required: 75
  NPC Start: 90, NPC Complete: 97
  Rewards: EXP=2100, Gold=1000
]]

local fsm = require('fsm_engine')

local quest_407 = fsm:new({
    id = 407,
    name = "[Quest] A village in ruins",
    level_required = 75,
    npc_start = 90,
    npc_complete = 97,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 407: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 407: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 407: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 407: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 407: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 407: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 407: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 407: Rewards given")
                self:give_rewards({
                    exp = 2100,
                    gold = 1000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_407