--[[
  Quest 551: [Repeat][Quest] Nosy Centaurs
  Level Required: 150
  NPC Start: 98, NPC Complete: 98
  Prerequisites: [550]
  Rewards: EXP=1000000
]]

local fsm = require('fsm_engine')

local quest_551 = fsm:new({
    id = 551,
    name = "[Repeat][Quest] Nosy Centaurs",
    level_required = 150,
    npc_start = 98,
    npc_complete = 98,
    prerequisites = {550},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 551: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 551: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 551: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 274,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 551: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 551: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 551: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 551: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 551: Rewards given")
                self:give_rewards({
                    exp = 1000000,
                    items = {
                        { item_id = 347, count = 1 },
                        { item_id = 30000048, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_551