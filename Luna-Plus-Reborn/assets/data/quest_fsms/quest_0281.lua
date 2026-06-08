--[[
  Quest 281: [Emblem of Knowledge] Dark Omens
  Level Required: 70
  NPC Start: 24, NPC Complete: 71
  Prerequisites: [280]
  Rewards: EXP=420000, Gold=20000
]]

local fsm = require('fsm_engine')

local quest_281 = fsm:new({
    id = 281,
    name = "[Emblem of Knowledge] Dark Omens",
    level_required = 70,
    npc_start = 24,
    npc_complete = 71,
    prerequisites = {280},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 281: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 281: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 281: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 281: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 71,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 281: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 281: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 71,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 281: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 281: Rewards given")
                self:give_rewards({
                    exp = 420000,
                    gold = 20000,
                    items = {
                        { item_id = 191, count = 1 },
                        { item_id = 191, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_281