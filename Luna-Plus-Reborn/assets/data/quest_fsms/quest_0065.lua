--[[
  Quest 65: [Wizard Job Change] Goblin Unity
  Level Required: 20
  NPC Start: 15, NPC Complete: 15
  Prerequisites: [50]
  Rewards: EXP=8610, Gold=1960
]]

local fsm = require('fsm_engine')

local quest_65 = fsm:new({
    id = 65,
    name = "[Wizard Job Change] Goblin Unity",
    level_required = 20,
    npc_start = 15,
    npc_complete = 15,
    prerequisites = {50},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 65: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 65: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 65: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 33,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 65: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 42,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 65: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 65: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 65: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 65: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 65: Rewards given")
                self:give_rewards({
                    exp = 8610,
                    gold = 1960,
                    items = {
                        { item_id = 21000008, count = 15 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_65