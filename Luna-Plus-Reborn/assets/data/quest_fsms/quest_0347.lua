--[[
  Quest 347: [B Class] Skeleton Knight Elimination
  Level Required: 57
  NPC Start: 83, NPC Complete: 83
  Prerequisites: [345]
  Rewards: EXP=531156
]]

local fsm = require('fsm_engine')

local quest_347 = fsm:new({
    id = 347,
    name = "[B Class] Skeleton Knight Elimination",
    level_required = 57,
    npc_start = 83,
    npc_complete = 83,
    prerequisites = {345},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 347: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 347: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 347: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 62,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 347: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 347: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 347: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 347: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 347: Rewards given")
                self:give_rewards({
                    exp = 531156,
                    items = {
                        { item_id = 253, count = 50 },
                        { item_id = 30000501, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_347