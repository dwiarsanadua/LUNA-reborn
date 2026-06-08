--[[
  Quest 268: [Warrior Job Change] Compensation (Sword)
  Level Required: 20
  NPC Start: 11, NPC Complete: 11
  Prerequisites: [54]
  Rewards: EXP=4149, Gold=550
]]

local fsm = require('fsm_engine')

local quest_268 = fsm:new({
    id = 268,
    name = "[Warrior Job Change] Compensation (Sword)",
    level_required = 20,
    npc_start = 11,
    npc_complete = 11,
    prerequisites = {54},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 268: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 268: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 268: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 268: NPC talk objective met")
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
                self:log("Quest 268: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 268: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 268: Rewards given")
                self:give_rewards({
                    exp = 4149,
                    gold = 550,
                    items = {
                        { item_id = 11000098, count = 1 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_268