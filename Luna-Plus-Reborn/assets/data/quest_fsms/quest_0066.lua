--[[
  Quest 66: [Wizard Job Change] Examination Results
  Level Required: 20
  NPC Start: 7, NPC Complete: 15
  Prerequisites: [65]
  Rewards: EXP=468, Gold=550
]]

local fsm = require('fsm_engine')

local quest_66 = fsm:new({
    id = 66,
    name = "[Wizard Job Change] Examination Results",
    level_required = 20,
    npc_start = 7,
    npc_complete = 15,
    prerequisites = {65},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 66: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 66: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 66: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 66: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 66: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 66: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 66: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 66: Rewards given")
                self:give_rewards({
                    exp = 468,
                    gold = 550,
                    items = {
                        { item_id = 11004996, count = 1 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_66