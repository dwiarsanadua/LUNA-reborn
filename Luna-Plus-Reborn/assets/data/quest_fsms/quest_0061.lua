--[[
  Quest 61: [Ruffian Job Change] Request Complete
  Level Required: 20
  NPC Start: 11, NPC Complete: 21
  Prerequisites: [60]
  Rewards: EXP=468, Gold=550
]]

local fsm = require('fsm_engine')

local quest_61 = fsm:new({
    id = 61,
    name = "[Ruffian Job Change] Request Complete",
    level_required = 20,
    npc_start = 11,
    npc_complete = 21,
    prerequisites = {60},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 61: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 61: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 61: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 61: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 61: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 61: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 61: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 61: Rewards given")
                self:give_rewards({
                    exp = 468,
                    gold = 550,
                    items = {
                        { item_id = 11002144, count = 1 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_61