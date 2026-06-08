--[[
  Quest 12: Traces of Battle
  Level Required: 14
  NPC Start: 15, NPC Complete: 20
  Prerequisites: [11]
  Rewards: EXP=299, Gold=500
]]

local fsm = require('fsm_engine')

local quest_12 = fsm:new({
    id = 12,
    name = "Traces of Battle",
    level_required = 14,
    npc_start = 15,
    npc_complete = 20,
    prerequisites = {11},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 12: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 12: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 12: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 12: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 12: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 12: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 12: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 12: Rewards given")
                self:give_rewards({
                    exp = 299,
                    gold = 500,
                    items = {
                        { item_id = 6, count = 1 },
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_12