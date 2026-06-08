--[[
  Quest 63: [Cleric Job Change] The Moral Ability
  Level Required: 20
  NPC Start: 14, NPC Complete: 18
  Prerequisites: [62]
  Rewards: EXP=7130, Gold=1775
]]

local fsm = require('fsm_engine')

local quest_63 = fsm:new({
    id = 63,
    name = "[Cleric Job Change] The Moral Ability",
    level_required = 20,
    npc_start = 14,
    npc_complete = 18,
    prerequisites = {62},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 63: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 18,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 63: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 63: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 48,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 63: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 49,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 63: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 18,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 63: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 63: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 18,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 63: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 63: Rewards given")
                self:give_rewards({
                    exp = 7130,
                    gold = 1775,
                    items = {
                        { item_id = 21000009, count = 15 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_63