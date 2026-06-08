--[[
  Quest 491: Harpy Queen
  Level Required: 41
  NPC Start: 38, NPC Complete: 38
  Rewards: EXP=228110, Gold=82647
]]

local fsm = require('fsm_engine')

local quest_491 = fsm:new({
    id = 491,
    name = "Harpy Queen",
    level_required = 41,
    npc_start = 38,
    npc_complete = 38,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 491: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 38,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 491: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 491: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 393,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 491: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 38,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 491: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 491: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 38,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 491: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 491: Rewards given")
                self:give_rewards({
                    exp = 228110,
                    gold = 82647,
                    items = {
                        { item_id = 332, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_491