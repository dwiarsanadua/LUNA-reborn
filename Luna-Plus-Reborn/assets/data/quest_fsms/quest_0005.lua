--[[
  Quest 5: Search Patrol
  Level Required: 7
  NPC Start: 6, NPC Complete: 13
  Rewards: EXP=189, Gold=530
]]

local fsm = require('fsm_engine')

local quest_5 = fsm:new({
    id = 5,
    name = "Search Patrol",
    level_required = 7,
    npc_start = 6,
    npc_complete = 13,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 5: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 5: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 5: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 5: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 5: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 5: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 5: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 5: Rewards given")
                self:give_rewards({
                    exp = 189,
                    gold = 530,
                    items = {
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_5