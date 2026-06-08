--[[
  Quest 315: [C Class] Predator Gargoyle Elimination
  Level Required: 44
  NPC Start: 80, NPC Complete: 80
  Prerequisites: [313]
  Rewards: EXP=180133, Gold=4700
]]

local fsm = require('fsm_engine')

local quest_315 = fsm:new({
    id = 315,
    name = "[C Class] Predator Gargoyle Elimination",
    level_required = 44,
    npc_start = 80,
    npc_complete = 80,
    prerequisites = {313},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 315: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 315: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 315: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 60,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 315: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 315: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 315: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 315: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 315: Rewards given")
                self:give_rewards({
                    exp = 180133,
                    gold = 4700,
                    items = {
                        { item_id = 220, count = 50 },
                        { item_id = 30000499, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_315