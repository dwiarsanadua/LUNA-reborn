--[[
  Quest 781: Dina's Case
  Level Required: 0
  NPC Start: 28, NPC Complete: 28
  Prerequisites: [780, 778, 779]
  Rewards: EXP=49916, Gold=2901
]]

local fsm = require('fsm_engine')

local quest_781 = fsm:new({
    id = 781,
    name = "Dina's Case",
    level_required = 0,
    npc_start = 28,
    npc_complete = 28,
    prerequisites = {780, 778, 779},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 781: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 781: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 781: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 781: NPC talk objective met")
                    end
                },
                {
                    trigger = "any",
                    target = "COMPLETE",
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 781: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 781: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 781: Rewards given")
                self:give_rewards({
                    exp = 49916,
                    gold = 2901,
                    items = {
                        { item_id = 21000008, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_781