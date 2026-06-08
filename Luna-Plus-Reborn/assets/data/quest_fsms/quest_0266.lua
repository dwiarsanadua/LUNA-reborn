--[[
  Quest 266: [Guard Job Change] Compensation (Mace)
  Level Required: 20
  NPC Start: 1, NPC Complete: 1
  Prerequisites: [245]
  Rewards: EXP=4149, Gold=550
]]

local fsm = require('fsm_engine')

local quest_266 = fsm:new({
    id = 266,
    name = "[Guard Job Change] Compensation (Mace)",
    level_required = 20,
    npc_start = 1,
    npc_complete = 1,
    prerequisites = {245},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 266: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 266: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 266: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 266: NPC talk objective met")
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
                self:log("Quest 266: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 266: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 266: Rewards given")
                self:give_rewards({
                    exp = 4149,
                    gold = 550,
                    items = {
                        { item_id = 11000067, count = 1 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_266