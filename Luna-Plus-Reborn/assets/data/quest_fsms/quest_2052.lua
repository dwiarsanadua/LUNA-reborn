--[[
  Quest 2052: [Daily] Rare Collections 2
  Level Required: 102
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=2711135
]]

local fsm = require('fsm_engine')

local quest_2052 = fsm:new({
    id = 2052,
    name = "[Daily] Rare Collections 2",
    level_required = 102,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2052: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2052: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2052: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2052: NPC talk objective met")
                    end
                },
                {
                    trigger = "any",
                    target = "COMPLETE",
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2052: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2052: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2052: Rewards given")
                self:give_rewards({
                    exp = 2711135,
                    items = {
                        { item_id = 30000836, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2052