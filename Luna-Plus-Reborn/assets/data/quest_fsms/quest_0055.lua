--[[
  Quest 55: [Warrior Job Change] Soothing The Spirits
  Level Required: 20
  NPC Start: 19, NPC Complete: 19
  Prerequisites: [53]
  Rewards: EXP=11415, Gold=2310
]]

local fsm = require('fsm_engine')

local quest_55 = fsm:new({
    id = 55,
    name = "[Warrior Job Change] Soothing The Spirits",
    level_required = 20,
    npc_start = 19,
    npc_complete = 19,
    prerequisites = {53},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 55: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 55: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 55: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 9,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 55: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 55: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 55: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 55: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 55: Rewards given")
                self:give_rewards({
                    exp = 11415,
                    gold = 2310,
                    items = {
                        { item_id = 19, count = 1 },
                        { item_id = 19, count = 30 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_55