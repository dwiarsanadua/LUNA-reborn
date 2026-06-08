--[[
  Quest 49: [Cleric Job Change] The Fearful Ability
  Level Required: 20
  NPC Start: 7, NPC Complete: 9
  Rewards: EXP=230, Gold=500
]]

local fsm = require('fsm_engine')

local quest_49 = fsm:new({
    id = 49,
    name = "[Cleric Job Change] The Fearful Ability",
    level_required = 20,
    npc_start = 7,
    npc_complete = 9,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 49: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 49: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 49: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 49: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 49: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 49: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 49: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 49: Rewards given")
                self:give_rewards({
                    exp = 230,
                    gold = 500,
                    items = {
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_49