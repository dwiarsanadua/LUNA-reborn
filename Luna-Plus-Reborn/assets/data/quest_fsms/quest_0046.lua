--[[
  Quest 46: [Warrior Job Change] A Legendary Soldier
  Level Required: 20
  NPC Start: 5, NPC Complete: 10
  Rewards: EXP=230, Gold=500
]]

local fsm = require('fsm_engine')

local quest_46 = fsm:new({
    id = 46,
    name = "[Warrior Job Change] A Legendary Soldier",
    level_required = 20,
    npc_start = 5,
    npc_complete = 10,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 46: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 46: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 46: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 46: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 46: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 46: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 46: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 46: Rewards given")
                self:give_rewards({
                    exp = 230,
                    gold = 500,
                    items = {
                        { item_id = 21000009, count = 9 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_46