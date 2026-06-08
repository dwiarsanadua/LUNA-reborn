--[[
  Quest 885: Dupaero Split Friendship
  Level Required: 95
  NPC Start: 535, NPC Complete: 535
  Rewards: EXP=7022796, Gold=77721
]]

local fsm = require('fsm_engine')

local quest_885 = fsm:new({
    id = 885,
    name = "Dupaero Split Friendship",
    level_required = 95,
    npc_start = 535,
    npc_complete = 535,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 885: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 885: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 885: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 291,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 885: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 885: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 885: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 885: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 885: Rewards given")
                self:give_rewards({
                    exp = 7022796,
                    gold = 77721,
                    items = {
                        { item_id = 30000685, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_885