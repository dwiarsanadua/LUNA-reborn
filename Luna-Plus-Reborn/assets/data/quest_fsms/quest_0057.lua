--[[
  Quest 57: [Voyager Job Change] Traces of The Star Seekers
  Level Required: 20
  NPC Start: 13, NPC Complete: 20
  Prerequisites: [47]
  Rewards: EXP=290, Gold=530
]]

local fsm = require('fsm_engine')

local quest_57 = fsm:new({
    id = 57,
    name = "[Voyager Job Change] Traces of The Star Seekers",
    level_required = 20,
    npc_start = 13,
    npc_complete = 20,
    prerequisites = {47},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 57: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 57: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 57: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 57: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 57: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 57: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 57: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 57: Rewards given")
                self:give_rewards({
                    exp = 290,
                    gold = 530,
                    items = {
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_57