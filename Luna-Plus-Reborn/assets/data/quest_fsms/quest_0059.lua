--[[
  Quest 59: [Voyager Job Change] Proof of The Star Seekers
  Level Required: 20
  NPC Start: 11, NPC Complete: 20
  Prerequisites: [58]
  Rewards: EXP=468, Gold=550
]]

local fsm = require('fsm_engine')

local quest_59 = fsm:new({
    id = 59,
    name = "[Voyager Job Change] Proof of The Star Seekers",
    level_required = 20,
    npc_start = 11,
    npc_complete = 20,
    prerequisites = {58},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 59: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 59: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 59: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 59: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 59: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 59: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 59: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 59: Rewards given")
                self:give_rewards({
                    exp = 468,
                    gold = 550,
                    items = {
                        { item_id = 11002175, count = 1 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_59