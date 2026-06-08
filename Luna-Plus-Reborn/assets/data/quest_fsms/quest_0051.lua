--[[
  Quest 51: [Guard Job Change] Needy Knights
  Level Required: 20
  NPC Start: 2, NPC Complete: 2
  Prerequisites: [45]
  Rewards: EXP=6660, Gold=1725
]]

local fsm = require('fsm_engine')

local quest_51 = fsm:new({
    id = 51,
    name = "[Guard Job Change] Needy Knights",
    level_required = 20,
    npc_start = 2,
    npc_complete = 2,
    prerequisites = {45},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 51: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 51: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 51: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 41,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 51: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 51: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 51: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 51: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 51: Rewards given")
                self:give_rewards({
                    exp = 6660,
                    gold = 1725,
                    items = {
                        { item_id = 18, count = 20 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_51