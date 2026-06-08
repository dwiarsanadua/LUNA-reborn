--[[
  Quest 62: [Cleric Job Change] The Purposeful Ability
  Level Required: 20
  NPC Start: 9, NPC Complete: 18
  Prerequisites: [49]
  Rewards: EXP=360, Gold=550
]]

local fsm = require('fsm_engine')

local quest_62 = fsm:new({
    id = 62,
    name = "[Cleric Job Change] The Purposeful Ability",
    level_required = 20,
    npc_start = 9,
    npc_complete = 18,
    prerequisites = {49},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 62: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 62: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 62: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 62: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 18,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 62: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 62: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 18,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 62: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 62: Rewards given")
                self:give_rewards({
                    exp = 360,
                    gold = 550,
                    items = {
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_62