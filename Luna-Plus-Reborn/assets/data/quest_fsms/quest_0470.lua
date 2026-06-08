--[[
  Quest 470: [Pet] Floating Stone Mystery
  Level Required: 150
  NPC Start: 7, NPC Complete: 34
  Rewards: EXP=2600, Gold=5000
]]

local fsm = require('fsm_engine')

local quest_470 = fsm:new({
    id = 470,
    name = "[Pet] Floating Stone Mystery",
    level_required = 150,
    npc_start = 7,
    npc_complete = 34,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 470: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 470: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 470: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 470: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 470: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 470: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 470: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 470: Rewards given")
                self:give_rewards({
                    exp = 2600,
                    gold = 5000,
                    items = {
                        { item_id = 329, count = 1 },
                        { item_id = 330, count = 1 },
                        { item_id = 329, count = 1 },
                        { item_id = 330, count = 1 },
                        { item_id = 21000420, count = 1 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_470