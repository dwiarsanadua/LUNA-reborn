--[[
  Quest 54: [Warrior Job Change] An Old Soldier's Respect
  Level Required: 20
  NPC Start: 5, NPC Complete: 10
  Prerequisites: [56]
  Rewards: EXP=468, Gold=550
]]

local fsm = require('fsm_engine')

local quest_54 = fsm:new({
    id = 54,
    name = "[Warrior Job Change] An Old Soldier's Respect",
    level_required = 20,
    npc_start = 5,
    npc_complete = 10,
    prerequisites = {56},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 54: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 54: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 54: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 54: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 54: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 54: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 54: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 54: Rewards given")
                self:give_rewards({
                    exp = 468,
                    gold = 550,
                    items = {
                        { item_id = 21000007, count = 15 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_54