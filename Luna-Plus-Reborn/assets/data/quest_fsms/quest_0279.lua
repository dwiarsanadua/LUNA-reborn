--[[
  Quest 279: [Emblem of Spirit] Stranded!
  Level Required: 70
  NPC Start: 55, NPC Complete: 70
  Prerequisites: [278]
  Rewards: EXP=651000, Gold=10000
]]

local fsm = require('fsm_engine')

local quest_279 = fsm:new({
    id = 279,
    name = "[Emblem of Spirit] Stranded!",
    level_required = 70,
    npc_start = 55,
    npc_complete = 70,
    prerequisites = {278},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 279: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 70,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 279: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 279: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 70,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 279: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 279: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 279: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 70,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 279: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 279: Rewards given")
                self:give_rewards({
                    exp = 651000,
                    gold = 10000,
                    items = {
                        { item_id = 187, count = 1 },
                        { item_id = 187, count = 1 },
                        { item_id = 30000055, count = 50 },
                        { item_id = 30000053, count = 50 },
                        { item_id = 30000056, count = 30 },
                        { item_id = 30000191, count = 20 },
                        { item_id = 188, count = 1 },
                        { item_id = 188, count = 1 },
                        { item_id = 30000483, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_279