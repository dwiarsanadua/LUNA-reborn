--[[
  Quest 376: [A Class] Two Shades of Evil
  Level Required: 69
  NPC Start: 94, NPC Complete: 94
  Prerequisites: [374]
  Rewards: EXP=1073249
]]

local fsm = require('fsm_engine')

local quest_376 = fsm:new({
    id = 376,
    name = "[A Class] Two Shades of Evil",
    level_required = 69,
    npc_start = 94,
    npc_complete = 94,
    prerequisites = {374},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 376: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 94,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 376: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 376: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 28,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 376: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 64,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 376: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 94,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 376: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 376: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 94,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 376: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 376: Rewards given")
                self:give_rewards({
                    exp = 1073249,
                    items = {
                        { item_id = 30000503, count = 1 },
                        { item_id = 30000312, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_376