--[[
  Quest 139: Louis Lua's request
  Level Required: 22
  NPC Start: 58, NPC Complete: 61
  Prerequisites: [113]
  Rewards: EXP=11879, Gold=35636
]]

local fsm = require('fsm_engine')

local quest_139 = fsm:new({
    id = 139,
    name = "Louis Lua's request",
    level_required = 22,
    npc_start = 58,
    npc_complete = 61,
    prerequisites = {113},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 139: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 61,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 139: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 139: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 52,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 139: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 61,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 139: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 58,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 139: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 139: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 61,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 139: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 139: Rewards given")
                self:give_rewards({
                    exp = 11879,
                    gold = 35636,
                    items = {
                        { item_id = 52, count = 1 },
                        { item_id = 52, count = 5 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_139