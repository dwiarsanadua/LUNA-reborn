--[[
  Quest 822: Neutralization Test
  Level Required: 75
  NPC Start: 92, NPC Complete: 92
  Rewards: EXP=2043968, Gold=61514
]]

local fsm = require('fsm_engine')

local quest_822 = fsm:new({
    id = 822,
    name = "Neutralization Test",
    level_required = 75,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 822: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 822: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 822: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 259,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 822: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 822: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 822: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 822: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 822: Rewards given")
                self:give_rewards({
                    exp = 2043968,
                    gold = 61514,
                    items = {
                        { item_id = 387, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_822