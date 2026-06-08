--[[
  Quest 933: Leaving in Silence
  Level Required: 103
  NPC Start: 536, NPC Complete: 536
  Rewards: EXP=15814408
]]

local fsm = require('fsm_engine')

local quest_933 = fsm:new({
    id = 933,
    name = "Leaving in Silence",
    level_required = 103,
    npc_start = 536,
    npc_complete = 536,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 933: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 933: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 933: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 296,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 933: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 933: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 933: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 933: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 933: Rewards given")
                self:give_rewards({
                    exp = 15814408,
                    items = {
                        { item_id = 407, count = 1 },
                        { item_id = 407, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_933