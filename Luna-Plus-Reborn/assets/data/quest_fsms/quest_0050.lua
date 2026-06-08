--[[
  Quest 50: [Wizard Job Change] Admission Examination
  Level Required: 20
  NPC Start: 7, NPC Complete: 15
  Rewards: EXP=360, Gold=550
]]

local fsm = require('fsm_engine')

local quest_50 = fsm:new({
    id = 50,
    name = "[Wizard Job Change] Admission Examination",
    level_required = 20,
    npc_start = 7,
    npc_complete = 15,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 50: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 50: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 50: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 50: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 50: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 50: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 50: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 50: Rewards given")
                self:give_rewards({
                    exp = 360,
                    gold = 550,
                })
            end,
            transitions = {},
        },
    },
})

return quest_50