--[[
  Quest 280: [Emblem of Knowledge] Dark Omens
  Level Required: 70
  NPC Start: 24, NPC Complete: 34
  Rewards: EXP=420000, Gold=20000
]]

local fsm = require('fsm_engine')

local quest_280 = fsm:new({
    id = 280,
    name = "[Emblem of Knowledge] Dark Omens",
    level_required = 70,
    npc_start = 24,
    npc_complete = 34,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 280: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 280: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 280: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 16,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 280: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 63,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 280: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 280: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 280: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 280: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 280: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 280: Rewards given")
                self:give_rewards({
                    exp = 420000,
                    gold = 20000,
                    items = {
                        { item_id = 189, count = 1 },
                        { item_id = 189, count = 1 },
                        { item_id = 190, count = 1 },
                        { item_id = 190, count = 1 },
                        { item_id = 190, count = 25 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_280