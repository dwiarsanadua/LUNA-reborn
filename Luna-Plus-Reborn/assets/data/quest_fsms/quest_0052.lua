--[[
  Quest 52: [Guard Job Change] Support Zakandia
  Level Required: 20
  NPC Start: 2, NPC Complete: 2
  Prerequisites: [51]
  Rewards: EXP=13760, Gold=2505
]]

local fsm = require('fsm_engine')

local quest_52 = fsm:new({
    id = 52,
    name = "[Guard Job Change] Support Zakandia",
    level_required = 20,
    npc_start = 2,
    npc_complete = 2,
    prerequisites = {51},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 52: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 52: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 52: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 42,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 52: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 49,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 52: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 48,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 52: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 52: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 52: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 52: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 52: Rewards given")
                self:give_rewards({
                    exp = 13760,
                    gold = 2505,
                    items = {
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_52