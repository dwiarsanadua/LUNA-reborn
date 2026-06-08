--[[
  Quest 147: [Repeatable] Spiritual Rest
  Level Required: 20
  NPC Start: 19, NPC Complete: 19
  Prerequisites: [93]
  Rewards: EXP=6222, Gold=4140
]]

local fsm = require('fsm_engine')

local quest_147 = fsm:new({
    id = 147,
    name = "[Repeatable] Spiritual Rest",
    level_required = 20,
    npc_start = 19,
    npc_complete = 19,
    prerequisites = {93},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 147: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 147: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 147: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 42,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 147: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 34,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 147: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 147: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 147: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 147: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 147: Rewards given")
                self:give_rewards({
                    exp = 6222,
                    gold = 4140,
                    items = {
                        { item_id = 21000008, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_147