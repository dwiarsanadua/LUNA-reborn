--[[
  Quest 56: [Warrior Job Change] An Old Soldier's Admiration
  Level Required: 20
  NPC Start: 5, NPC Complete: 19
  Prerequisites: [55]
  Rewards: EXP=360, Gold=550
]]

local fsm = require('fsm_engine')

local quest_56 = fsm:new({
    id = 56,
    name = "[Warrior Job Change] An Old Soldier's Admiration",
    level_required = 20,
    npc_start = 5,
    npc_complete = 19,
    prerequisites = {55},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 56: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 56: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 56: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 56: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 56: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 56: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 56: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 56: Rewards given")
                self:give_rewards({
                    exp = 360,
                    gold = 550,
                    items = {
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_56