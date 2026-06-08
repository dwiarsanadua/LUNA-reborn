--[[
  Quest 154: The Training Begins
  Level Required: 27
  NPC Start: 29, NPC Complete: 29
  Prerequisites: [127]
  Rewards: EXP=19420, Gold=3240
]]

local fsm = require('fsm_engine')

local quest_154 = fsm:new({
    id = 154,
    name = "The Training Begins",
    level_required = 27,
    npc_start = 29,
    npc_complete = 29,
    prerequisites = {127},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 154: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 154: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 154: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 50,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 154: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 154: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 154: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 154: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 154: Rewards given")
                self:give_rewards({
                    exp = 19420,
                    gold = 3240,
                    items = {
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_154