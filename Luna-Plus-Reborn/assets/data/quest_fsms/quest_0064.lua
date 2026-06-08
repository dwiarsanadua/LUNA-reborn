--[[
  Quest 64: [Cleric Job Change] Becoming a Cleric
  Level Required: 20
  NPC Start: 9, NPC Complete: 18
  Prerequisites: [63]
  Rewards: EXP=468, Gold=550
]]

local fsm = require('fsm_engine')

local quest_64 = fsm:new({
    id = 64,
    name = "[Cleric Job Change] Becoming a Cleric",
    level_required = 20,
    npc_start = 9,
    npc_complete = 18,
    prerequisites = {63},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 64: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 18,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 64: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 64: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 18,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 64: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 64: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 64: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 18,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 64: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 64: Rewards given")
                self:give_rewards({
                    exp = 468,
                    gold = 550,
                    items = {
                        { item_id = 11006391, count = 1 },
                        { item_id = 21000008, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_64