--[[
  Quest 729: [Quest] Log Union
  Level Required: 51
  NPC Start: 11, NPC Complete: 119
  Prerequisites: [728]
  Rewards: EXP=317002
]]

local fsm = require('fsm_engine')

local quest_729 = fsm:new({
    id = 729,
    name = "[Quest] Log Union",
    level_required = 51,
    npc_start = 11,
    npc_complete = 119,
    prerequisites = {728},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 729: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 729: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 729: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 285,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 729: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 87,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 729: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 729: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 729: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 729: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 729: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 729: Rewards given")
                self:give_rewards({
                    exp = 317002,
                    items = {
                        { item_id = 359, count = 1 },
                        { item_id = 359, count = 1 },
                        { item_id = 359, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_729