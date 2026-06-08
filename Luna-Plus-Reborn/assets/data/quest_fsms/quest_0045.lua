--[[
  Quest 45: [Guard Job Change] Duty of The Detachment
  Level Required: 20
  NPC Start: 2, NPC Complete: 10
  Rewards: EXP=290, Gold=530
]]

local fsm = require('fsm_engine')

local quest_45 = fsm:new({
    id = 45,
    name = "[Guard Job Change] Duty of The Detachment",
    level_required = 20,
    npc_start = 2,
    npc_complete = 10,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 45: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 45: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 45: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 45: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 45: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 45: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 45: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 45: Rewards given")
                self:give_rewards({
                    exp = 290,
                    gold = 530,
                    items = {
                        { item_id = 21000009, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_45