--[[
  Quest 842: Curse of the Black Widow 2
  Level Required: 85
  NPC Start: 126, NPC Complete: 126
  Prerequisites: [841]
  Rewards: EXP=3227772, Gold=57826
]]

local fsm = require('fsm_engine')

local quest_842 = fsm:new({
    id = 842,
    name = "Curse of the Black Widow 2",
    level_required = 85,
    npc_start = 126,
    npc_complete = 126,
    prerequisites = {841},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 842: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 842: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 842: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 281,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 842: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 842: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 842: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 842: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 842: Rewards given")
                self:give_rewards({
                    exp = 3227772,
                    gold = 57826,
                })
            end,
            transitions = {},
        },
    },
})

return quest_842